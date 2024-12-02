<template>
  <div class="advanced-search">
    <div class="search-form">
      <div class="search-input-group">
        <input 
          v-model="searchParams.query"
          type="text"
          placeholder="Поиск сообщений..."
          @input="handleSearch"
        >
        <button 
          class="toggle-filters"
          @click="showFilters = !showFilters"
        >
          {{ showFilters ? 'Скрыть фильтры' : 'Показать фильтры' }}
        </button>
      </div>

      <div v-if="showFilters" class="filters">
        <div class="filter-group">
          <label>От кого:</label>
          <select v-model="searchParams.userId">
            <option value="">Все пользователи</option>
            <option 
              v-for="user in users" 
              :key="user.id" 
              :value="user.id"
            >
              {{ user.name }}
            </option>
          </select>
        </div>

        <div class="filter-group">
          <label>Период:</label>
          <div class="date-inputs">
            <input 
              type="date" 
              v-model="searchParams.dateFrom"
              :max="searchParams.dateTo"
            >
            <span>—</span>
            <input 
              type="date" 
              v-model="searchParams.dateTo"
              :min="searchParams.dateFrom"
            >
          </div>
        </div>
      </div>

      <div v-if="searchHistory.length" class="search-history">
        <h4>История поиска:</h4>
        <div class="history-items">
          <button 
            v-for="(query, index) in searchHistory" 
            :key="index"
            class="history-item"
            @click="useHistoryQuery(query)"
          >
            {{ query }}
          </button>
        </div>
      </div>
    </div>

    <div v-if="loading" class="loading">
      Поиск...
    </div>

    <div v-else-if="searchResults.length" class="search-results">
      <div 
        v-for="result in searchResults" 
        :key="result.id"
        class="search-result"
        @click="scrollToMessage(result.id)"
      >
        <div class="result-header">
          <span class="author">{{ result.user.name }}</span>
          <span class="time">{{ formatDateTime(result.created_at) }}</span>
        </div>
        <div class="result-content" v-html="highlightText(result.content)"></div>
      </div>
    </div>

    <div v-else-if="hasSearched" class="no-results">
      Ничего не найдено
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, watch } from 'vue'
import { debounce } from 'lodash'
import MessageService from '@/services/MessageService'

const searchParams = ref({
  query: '',
  userId: '',
  dateFrom: '',
  dateTo: ''
})

const showFilters = ref(false)
const users = ref([])
const searchResults = ref([])
const loading = ref(false)
const hasSearched = ref(false)
const searchHistory = ref([])

const emit = defineEmits(['scroll-to-message'])

const loadUsers = async () => {
  users.value = await MessageService.getUsers()
}

const handleSearch = debounce(async () => {
  if (!searchParams.value.query && !searchParams.value.userId && 
      !searchParams.value.dateFrom && !searchParams.value.dateTo) {
    searchResults.value = []
    hasSearched.value = false
    return
  }

  loading.value = true
  hasSearched.value = true

  try {
    const { messages } = await MessageService.searchMessages({
      query: searchParams.value.query,
      user_id: searchParams.value.userId,
      date_from: searchParams.value.dateFrom,
      date_to: searchParams.value.dateTo
    })
    
    searchResults.value = messages
    
    if (searchParams.value.query) {
      MessageService.saveSearchHistory(searchParams.value.query)
      loadSearchHistory()
    }
  } finally {
    loading.value = false
  }
}, 300)

const loadSearchHistory = () => {
  searchHistory.value = MessageService.getSearchHistory()
}

const useHistoryQuery = (query) => {
  searchParams.value.query = query
  handleSearch()
}

const formatDateTime = (timestamp) => {
  return new Date(timestamp).toLocaleString()
}

const highlightText = (text) => {
  if (!searchParams.value.query) return text
  const regex = new RegExp(`(${searchParams.value.query})`, 'gi')
  return text.replace(regex, '<mark>$1</mark>')
}

const scrollToMessage = (messageId) => {
  emit('scroll-to-message', messageId)
}

watch(searchParams, handleSearch, { deep: true })

onMounted(() => {
  loadUsers()
  loadSearchHistory()
})
</script>

<style scoped>
.advanced-search {
  margin-bottom: 20px;
}

.search-form {
  background: white;
  padding: 15px;
  border-radius: 8px;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}

.search-input-group {
  display: flex;
  gap: 10px;
  margin-bottom: 10px;
}

input[type="text"] {
  flex: 1;
  padding: 8px;
  border: 1px solid #ddd;
  border-radius: 4px;
}

.toggle-filters {
  padding: 8px 16px;
  background: #f0f0f0;
  border: none;
  border-radius: 4px;
  cursor: pointer;
}

.filters {
  padding: 15px;
  background: #f5f5f5;
  border-radius: 4px;
  margin-bottom: 15px;
}

.filter-group {
  margin-bottom: 10px;
}

.filter-group label {
  display: block;
  margin-bottom: 5px;
  color: #666;
}

.date-inputs {
  display: flex;
  align-items: center;
  gap: 10px;
}

.search-history {
  margin-top: 15px;
  padding-top: 15px;
  border-top: 1px solid #eee;
}

.history-items {
  display: flex;
  flex-wrap: wrap;
  gap: 8px;
  margin-top: 8px;
}

.history-item {
  padding: 4px 8px;
  background: #f0f0f0;
  border: none;
  border-radius: 4px;
  cursor: pointer;
  font-size: 0.9em;
}

.search-results {
  margin-top: 15px;
}

.search-result {
  padding: 10px;
  background: white;
  border-radius: 4px;
  margin-bottom: 8px;
  cursor: pointer;
  transition: background-color 0.2s;
}

.search-result:hover {
  background: #f5f5f5;
}

.loading, .no-results {
  text-align: center;
  padding: 20px;
  color: #666;
}

:deep(mark) {
  background: #ffeb3b;
  padding: 0 2px;
  border-radius: 2px;
}
</style>