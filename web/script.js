let socket;
let reconnectTimer;
let statusElement;
let timeElement;
const reconnectDelay = 2000;

const statusIndicators = [
    {label: 'Builtin LED', variable_id: 'led-status', element_id: 'led-status'},
]

const statusVariableToElementMap = {};

for (const indicator of statusIndicators) {
    statusVariableToElementMap[indicator.variable_id] = indicator.element_id;
}

function resetStatusIndicators() {
    for (const indicator of statusIndicators) {
        const element = document.getElementById(indicator.element_id);
        if (element) {
            element.querySelector('.status-light').className = 'status-light';
        }
    }
}

function scheduleReconnect() {
    if (reconnectTimer) {
        return;
    }

    reconnectTimer = setTimeout(() => {
        reconnectTimer = undefined;
        connectSocket();
    }, reconnectDelay);
}

function connectSocket() {
    if (socket && (
        socket.readyState === WebSocket.CONNECTING ||
        socket.readyState === WebSocket.OPEN
    )) {
        return;
    }

    try {
        socket = new WebSocket(`ws://${window.location.hostname}:81/`);
    } catch (error) {
        console.warn('Unable to connect to the WebSocket server.', error);
        scheduleReconnect();
        return;
    }

    socket.addEventListener('open', () => {
        if (statusElement) {
            statusElement.textContent = 'Connected';
        }
    });

    socket.addEventListener('error', () => {
        if (statusElement) {
            statusElement.textContent = 'Connection error';
        }
        console.warn('WebSocket connection failed.');
    });

    socket.addEventListener('close', () => {
        if (statusElement) {
            statusElement.textContent = 'Disconnected';
        }
        resetStatusIndicators();
        scheduleReconnect();
    });

    socket.addEventListener('message', handleSocketMessage);
}

function handleSocketMessage(event) {
    console.log('Received WebSocket message:', event.data);
    const message = JSON.parse(event.data);

    if (message.type === 'time' && timeElement) {
        timeElement.textContent = `Arduino time: ${message.millis} ms`;

    }else if (message.type === 'status') {
        // map variable name to element id
        const indicator = document.getElementById(statusVariableToElementMap[message.variable]);
        if (indicator) {
            indicator.querySelector('.status-light').className = message.state ? 'status-light on' : 'status-light off';
        }else{
            if(message.variable === 'lift-state') {
                const liftStateStatusElement = document.getElementById('lift-state-status');
                if (liftStateStatusElement) {
                    liftStateStatusElement.textContent = message.state;
                }
            } else if(message.variable === 'station-state') {
                const stationStateStatusElement = document.getElementById('station-state-status');
                if (stationStateStatusElement) {
                    stationStateStatusElement.textContent = message.state;
                }
            } else if(message.variable === 'track-state') {
                const trackStateStatusElement = document.getElementById('track-state-status');
                if (trackStateStatusElement) {
                    trackStateStatusElement.textContent = message.state;
                }
            }
        }

    } else if (message.type === 'fault') {
        const faultStatusElement = document.getElementById('fault-status');
        if (faultStatusElement) {
            if (message.message && message.message !== 'cleared') {
                faultStatusElement.textContent = message.message;
                faultStatusElement.style.color = 'red';
            } else {
                faultStatusElement.textContent = 'None';
                faultStatusElement.style.color = 'black';
            }
        }

    }else if (message.type == 'ride_mode') {
        const rideModeStatusElement = document.getElementById('ride-mode-status');
        if (rideModeStatusElement) {
            rideModeStatusElement.textContent = message.mode === 0 ? 'Idle' : message.mode === 1 ? 'Running' : 'Fault';
        }
    }
}

function sendSocketMessage(message) {
    if (!socket || socket.readyState !== WebSocket.OPEN) {
        console.warn('WebSocket is not connected; message was not sent.');
        return false;
    }

    socket.send(JSON.stringify(message));
    return true;
}

function sendLedState(state) {
    sendSocketMessage({ action: 'led', state });
}

function addStatusIndicator(container_id, id, variableName) {
    const container = document.getElementById(container_id);
    if (!container) return;

    const indicator = document.createElement('div');
    indicator.id = id;
    indicator.className = 'status-indicator';
    container.appendChild(indicator);

    const label = document.createElement('span');
    label.textContent = variableName;
    label.className = 'status-label';
    indicator.appendChild(label);

    const status = document.createElement('div');
    status.className = 'status-light';
    indicator.appendChild(status);
}

window.addEventListener('DOMContentLoaded', () => {
    statusElement = document.getElementById('status');
    timeElement = document.getElementById('time');
    document.getElementById('led-on').addEventListener('click', () => {
        sendLedState(true);
    });
    document.getElementById('led-off').addEventListener('click', () => {
        sendLedState(false);
    });

    document.getElementById('begin_cycle').addEventListener('click', () => {
        sendSocketMessage({ action: 'request_start' });
    });

    for (const indicator of statusIndicators) {
        addStatusIndicator('status-indicator-container', indicator.element_id, indicator.label);
    }

    connectSocket();
});