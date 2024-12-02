<template>
  <div class="message-search">
    <div class="search-input">
      <input 
        v-model="searchQuery"
        type="text"
        placeholder="Поиск сообщений..."
        @input="handleSearch"
      >
      <button 
        v-if="searchQuery"
        @click="clearSearch"
        class="clear-button"
      >
        ✕
      </button>
    </div>
    
    <div v-if="isSearching" class="search-results">
      <div v-if="loading" class="loading">
        Поиск...
      </div>
      <div v-else-if="searchResults.length" class="results-list">
        <div 
          v-for="result in searchResults" 
          :key="result.id"
          class="search-result"
          @click="scrollToMessage(result.id)"
        >
          <div class="result-header">
            <span class="author">{{ result.user.name }}</span>
            <span class="time">{{ formatTime(result.created_at) }}</span>
          </div>
          <div class="result-content" v-html="highlightText(result.content)"></div>
        </div>
      </div>
      <div v-else-if="searchQuery" class="no-results">
        Ничего не найдено
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, watch } from 'vue'
import { debounce } from 'lodash'
import MessageService from '@/services/MessageService'

const searchQuery = ref('')
const searchResults = ref([])
const loading = ref(false)
const isSearching = ref(false)

const emit = defineEmits(['scroll-to-message'])

const handleSearch = debounce(async () => {
  if (!searchQuery.value.trim()) {
    clearSearch()
    return
  }

  loading.value = true
  isSearching.value = true

  try {
    const { messages } = await MessageService.searchMessages(searchQuery.value)
    searchResults.value = messages
  } finally {
    loading.value = false
  }
}, 300)

const clearSearch = () => {
  searchQuery.value = ''
  searchResults.value = []
  isSearching.value = false
}

const scrollToMessage = (messageId) => {
  emit('scroll-to-message', messageId)
  clearSearch()
}

const highlightText = (text) => {
  if (!searchQuery.value) return text
  const regex = new RegExp(`(${searchQuery.value})`, 'gi')
  return text.replace(regex, '<mark>$1</mark>')
}

const formatTime = (timestamp) => {
  return new Date(timestamp).toLocaleTimeString()
}
</script>

<style scoped>
.message-search {
  position: relative;
  margin-bottom: 15px;
}

.search-input {
  position: relative;
}

input {
  width: 100%;
  padding: 8px 30px 8px 12px;
  border: 1px solid #ddd;
  border-radius: 4px;
  font-size: 14px;
}

.clear-button {
  position: absolute;
  right: 8px;
  top: 50%;
  transform: translateY(-50%);
  background: none;
  border: none;
  cursor: pointer;
  color: #666;
}

.search-results {
  position: absolute;
  top: 100%;
  left: 0;
  right: 0;
  background: white;
  border: 1px solid #ddd;
  border-radius: 4px;
  max-height: 300px;
  overflow-y: auto;
  z-index: 1000;
  box-shadow: 0 2px 4px rgba(0,0,0,0.1);
}

.search-result {
  padding: 10px;
  cursor: pointer;
  border-bottom: 1px solid #eee;
}

.search-result:hover {
  background: #f5f5f5;
}

.result-header {
  display: flex;
  justify-content: space-between;
  font-size: 0.9em;
  margin-bottom: 5px;
}

.time {
  color: #666;
}

.loading, .no-results {
  padding: 15px;
  text-align: center;
  color: #666;
}

:deep(mark) {
  background: #ffeb3b;
  padding: 0 2px;
  border-radius: 2px;
}
</style> 