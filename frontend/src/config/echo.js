import Echo from 'laravel-echo'
import Pusher from 'pusher-js'

window.Pusher = Pusher

const echo = new Echo({
    broadcaster: 'pusher',
    key: 'local',
    wsHost: window.location.hostname,
    wsPort: 6001,
    forceTLS: false,
    disableStats: true,
    enabledTransports: ['ws', 'wss'],
    auth: {
        headers: {
            'Authorization': `Bearer ${localStorage.getItem('token')}`
        }
    }
})

export const updateEchoAuthToken = (token) => {
    echo.connector.options.auth.headers['Authorization'] = `Bearer ${token}`
}

export default echo 