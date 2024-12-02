import axios from 'axios'
import CacheService from './CacheService'

class MessageService {
    constructor() {
        this.pageSize = 50
        this.loading = false
        this.hasMore = true
        this.searchQueue = []
        this.processingSearch = false
    }

    async fetchMessages(page = 1) {
        if (this.loading || !this.hasMore) return []

        this.loading = true
        try {
            const response = await axios.get('/api/messages', {
                params: {
                    page,
                    per_page: this.pageSize
                }
            })

            this.hasMore = response.data.has_more
            return response.data.messages
        } catch (error) {
            console.error('Ошибка загрузки сообщений:', error)
            return []
        } finally {
            this.loading = false
        }
    }

    resetPagination() {
        this.hasMore = true
        this.loading = false
    }

    async searchMessages(params) {
        // Проверяем кэш
        const cached = CacheService.getSearchResult(params)
        if (cached) {
            return cached
        }

        // Добавляем запрос в очередь
        return new Promise((resolve, reject) => {
            this.searchQueue.push({ params, resolve, reject })
            this.processSearchQueue()
        })
    }

    async processSearchQueue() {
        if (this.processingSearch || this.searchQueue.length === 0) {
            return
        }

        this.processingSearch = true
        const { params, resolve, reject } = this.searchQueue.shift()

        try {
            const response = await axios.get('/api/messages/search', {
                params: {
                    ...params,
                    per_page: this.pageSize
                }
            })

            // Кэшируем результат
            CacheService.setSearchResult(params, response.data)
            resolve(response.data)
        } catch (error) {
            reject(error)
        } finally {
            this.processingSearch = false
            // Обрабатываем следующий запрос в очереди
            this.processSearchQueue()
        }
    }

    formatDateForApi(date) {
        if (!date) return null
        return date.toISOString().split('T')[0]
    }

    async getUsers() {
        const cached = CacheService.get('users')
        if (cached) {
            return cached
        }

        try {
            const response = await axios.get('/api/users')
            CacheService.set('users', response.data, 30 * 60 * 1000) // 30 минут
            return response.data
        } catch (error) {
            console.error('Ошибка получения пользователей:', error)
            return []
        }
    }

    saveSearchHistory(query) {
        const history = this.getSearchHistory()
        const newHistory = [query, ...history.filter(q => q !== query)].slice(0, 10)
        localStorage.setItem('searchHistory', JSON.stringify(newHistory))
    }

    getSearchHistory() {
        return JSON.parse(localStorage.getItem('searchHistory') || '[]')
    }

    groupMessagesByDate(messages) {
        return messages.reduce((groups, message) => {
            const date = new Date(message.created_at).toLocaleDateString()
            if (!groups[date]) {
                groups[date] = []
            }
            groups[date].push(message)
            return groups
        }, {})
    }
}

export default new MessageService() 