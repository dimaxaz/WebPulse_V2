# Sensor Monitoring System

Распределенная система мониторинга сенсоров с микросервисной архитектурой.

## Сервисы

### Frontend (Vue.js)
- SPA приложение
- Real-time обновления через WebSocket
- Интерактивные графики и дашборды
- Адаптивный дизайн
- Аутентификация и авторизация

### Admin Panel (PHP/Laravel)
- Административный интерфейс
- Управление пользователями и ролями
- Конфигурация системы
- Мониторинг и логирование
- API для внутренних сервисов

### Sensor Service (C++)
- Сбор данных с сенсоров
- Буферизация и предварительная обработка
- Мониторинг состояния сенсоров
- Профилирование производительности
- Метрики и трейсинг

### API Service (Python/FastAPI) 
- REST API для доступа к данным
- WebSocket для real-time обновлений
- Интеграция с Kafka
- Система безопасности и мониторинга
- Документация API

## Технологии

### Frontend
- Vue.js 3
- Vuex/Pinia
- Vue Router
- Axios
- Chart.js/D3.js
- TailwindCSS
- WebSocket

### Admin Panel
- PHP 8.1+
- Laravel 10
- Laravel Sanctum
- Laravel Horizon
- Livewire
- Blade Templates
- MySQL/PostgreSQL

### Backend
- C++ 17
- Python 3.9+
- FastAPI
- Apache Kafka
- Redis
- Elasticsearch

### Мониторинг
- Prometheus
- Grafana 
- Jaeger
- FlameGraph

### Безопасность
- JWT аутентификация
- API ключи
- Двухфакторная аутентификация
- Rate limiting
- Анализ угроз

### Требования
- Docker & Docker Compose
- Node.js 16+
- PHP 8.1+
- Composer
- CMake 3.15+
- Python 3.9+
- GCC 9+ или Clang 10+