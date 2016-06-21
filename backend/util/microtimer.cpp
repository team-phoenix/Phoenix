/*
 * Copyright © 2016 athairus
 *
 * This file is part of Phoenix.
 *
 * Phoenix is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "microtimer.h"

#include <QDateTime>
#include <QEvent>

#include "logging.h"

MicroTimer::MicroTimer( Node *parent ) : Node( parent ) {
    timer.invalidate();
    startFreq( 60.0 );
    qCDebug( phxTimer ) << "Beginning timer at a default frequency of 60Hz";
}

MicroTimer::~MicroTimer() {
    if( timer.isValid() || registeredTimers.size() ) {
        stop();
    }
}

// Public

bool MicroTimer::event( QEvent *e ) {
    // Abort if frequency hasn't been set yet, is negative or the timer is shut off
    if( coreFPS > 0 && timer.isValid() && e->type() == QEvent::Timer ) {
        // We use ms internally
        qreal currentTime = timer.nsecsElapsed() / 1000000.0;

        // Check to see if it's appropiate to send out a signal right now
        // If so, send the signal and recalculate the new target time
        if( currentTime >= targetTime ) {
            // Advance target time forward until it's at least one period past our current time
            int counter = 0;

            while( currentTime >= targetTime ) {
                targetTime += 1000.0 / coreFPS;
                counter++;
            }

            if( counter > 1 ) {
                //qCWarning( phxTimer ).nospace() << "Skipped " << counter - 1 << " frame(s)!";
                emit missedTimeouts( counter - 1 );
            }

            if( ( !vsync || state != State::Playing || !fpsDiffOkay() ) && globalPipelineReady ) {
                commandOut( Command::Heartbeat, QVariant(), nodeCurrentTime() );
            }

            // This situation should be dealt with as soon as possible... but can only be reliably dealt with once
            // the dynamic pipeline's established
            if( vsync && !fpsDiffOkay() && dynamicPipelineReady ) {
                vsync = false;
                qCDebug( phxTimer ) << "VSync enabled but coreFPS and hostFPS differ by at least 2%. Telling remainder of pipeline to assume VSync is off...";
                emit commandOut( Command::SetVsync, false, nodeCurrentTime() );
            }
        }

        return true;
    }

    return QObject::event( e );
}

// Public slots

void MicroTimer::startFreq( qreal frequency ) {
    if( frequency <= 0 ) {
        qWarning() << "MicroTimer: Frequency cannot be 0 or negative";
        return;
    }

    this->coreFPS = frequency;

    // Set inital target to the next interval
    targetTime = 1000.0 / frequency;

    // Kill all the timers
    killTimers();

    // Register these timers to generate as many events as possible to give us as many opportunities to check the freq counter as possible

    // Check time each time the window has run out of events to process
    // Causes heavy CPU usage
    if( maxAccuracy ) {
        registeredTimers << startTimer( 0, Qt::PreciseTimer );
    }

    // Check time every 1ms using native OS timers
    // Windows: Makes thread wake up every 1ms instead of the default 4ms (are you sure?)
    // OS X: Unix systems have ms accuracy
    // Linux: Unix systems have ms accuracy
    registeredTimers << startTimer( 1, Qt::PreciseTimer );

    // Begin tracking the current time
    timer.restart();
}

void MicroTimer::stop() {
    killTimers();
}

void MicroTimer::commandIn( Node::Command command, QVariant data, qint64 timeStamp ) {
    switch( command ) {
        // Always take over when the emulation has stopped or the global pipeline has been established as the window may
        // not emit heartbeats all the time... or at all
        case Command::Stop: {
            state = State::Stopped;
            emit commandOut( command, data, timeStamp );
            break;
        }

        case Command::Load: {
            state = State::Loading;
            emit commandOut( command, data, timeStamp );
            break;
        }

        case Command::Pause: {
            state = State::Paused;
            emit commandOut( command, data, timeStamp );
            break;
        }

        case Command::Unload: {
            state = State::Unloading;
            emit commandOut( command, data, timeStamp );
            break;
        }

        case Command::HandleGlobalPipelineReady: {
            globalPipelineReady = true;
            emit commandOut( command, data, timeStamp );
            break;
        }

        case Command::HandleDynamicPipelineReady: {
            dynamicPipelineReady = true;
            emit commandOut( command, data, timeStamp );
            break;
        }

        case Command::Play: {
            state = State::Playing;
            emit commandOut( command, data, timeStamp );
            break;
        }

        // Only let VSync through if:
        // - VSync on
        // - Playing
        // - coreFPS and hostFPS differ by less than 2%
        // - global pipeline is fully connected
        case Command::Heartbeat: {
            if( vsync && state == State::Playing && fpsDiffOkay() && globalPipelineReady ) {
                emit commandOut( command, data, timeStamp );
            }

            break;
        }

        case Command::SetHostFPS: {
            emit commandOut( command, data, timeStamp );
            hostFPS = data.toReal();
            break;
        }

        case Command::SetCoreFPS: {
            emit commandOut( command, data, timeStamp );
            qCDebug( phxTimer ).nospace() << "Began timer at " << data.toReal() << "Hz (vsync: " << vsync << ")";
            startFreq( data.toReal() );
            break;
        }

        // Invert vsync value to get what we should do
        case Command::SetVsync: {
            emit commandOut( command, data, timeStamp );
            vsync = data.toBool();
            break;
        }

        default:
            emit commandOut( command, data, timeStamp );
            break;
    }
}

void MicroTimer::killTimers() {
    timer.invalidate();

    for( int timerID : registeredTimers ) {
        killTimer( timerID );
    }

    registeredTimers.clear();
}

// Private

bool MicroTimer::fpsDiffOkay() {
    // If we're vsync'd and the discrepency is too large between coreFPS and hostFPS, do not use vsync'd
    // signals. 2% is a reasonable value for this.
    // This could happen if the user's monitor (hostFPS) is 120Hz and our game (coreFPS) is 60Hz
    // or the game is PAL (coreFPS = 50Hz) and the user's monitor (hostFPS) is 60Hz
    // If vsync is off, we'll always heartbeat at the correct rate
    qreal percentDifference = qAbs( hostFPS - coreFPS ) / coreFPS * 100;

    // Force vsync off from this node down
    // This will not affect PhoenixWindow
    if( percentDifference > 2.0 ) {
        return false;
    }

    return true;
}

void MicroTimer::setMaxAccuracy( bool maxAccuracy ) {
    this->maxAccuracy = maxAccuracy;

    // Reset to apply the change unless frequency is <= 0, in which case the thing's not running and we should just
    // kill the timers if they're running
    if( coreFPS > 0 ) {
        startFreq( coreFPS );
    } else {
        killTimers();
    }
}
