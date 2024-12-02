class NotificationService {
    constructor() {
        this.audio = new Audio('/notification.mp3');
        this.permission = 'default';
        this.init();
    }

    async init() {
        if ('Notification' in window) {
            this.permission = await Notification.requestPermission();
        }
    }

    playSound() {
        this.audio.play().catch(error => console.error('Ошибка воспроизведения:', error));
    }

    async notify(title, options = {}) {
        if (this.permission === 'granted') {
            const notification = new Notification(title, {
                icon: '/icon.png',
                ...options
            });

            this.playSound();

            notification.onclick = () => {
                window.focus();
                notification.close();
            };
        }
    }
}

export default new NotificationService(); 