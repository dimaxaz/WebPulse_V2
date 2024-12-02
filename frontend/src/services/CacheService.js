class CacheService {
    constructor() {
        this.cache = new Map()
        this.searchCache = new Map()
        this.TTL = 5 * 60 * 1000 // 5 минут
    }

    set(key, value, customTTL = null) {
        const ttl = customTTL || this.TTL
        const item = {
            value,
            timestamp: Date.now(),
            ttl
        }
        this.cache.set(key, item)
    }

    get(key) {
        const item = this.cache.get(key)
        if (!item) return null

        if (Date.now() - item.timestamp > item.ttl) {
            this.cache.delete(key)
            return null
        }

        return item.value
    }

    setSearchResult(params, results) {
        const key = this.createSearchKey(params)
        this.searchCache.set(key, {
            results,
            timestamp: Date.now()
        })

        // Ограничиваем размер кэша
        if (this.searchCache.size > 100) {
            const oldestKey = Array.from(this.searchCache.keys())[0]
            this.searchCache.delete(oldestKey)
        }
    }

    getSearchResult(params) {
        const key = this.createSearchKey(params)
        const cached = this.searchCache.get(key)

        if (!cached || Date.now() - cached.timestamp > this.TTL) {
            this.searchCache.delete(key)
            return null
        }

        return cached.results
    }

    createSearchKey(params) {
        return JSON.stringify(params)
    }

    clear() {
        this.cache.clear()
        this.searchCache.clear()
    }
}

export default new CacheService() 