<template>
  <div class="messages-container" ref="container">
    <MessageSearch @scroll-to-message="scrollToMessage" />
    
    <RecycleScroller
      class="messages-list"
      :items="groupedMessages"
      :item-size="getItemSize"
      key-field="id"
      v-slot="{ item }"
      @scroll="handleScroll"
    >
      <template v-if="item.type === 'date'">
        <div class="date-separator">
          {{ formatDate(item.date) }}
        </div>
      </template>
      <template v-else>
        <div 
          :id="`message-${item.id}`"
          class="message"
          :class="{ 'my-message': isMyMessage(item) }"
        >
          <div class="message-content">
            <div class="message-header">
              <span class="author">{{ item.user.name }}</span>
              <span class="time">{{ formatTime(item.created_at) }}</span>
            </div>
            <div class="message-text">{{ item.content }}</div>
            <div class="message-status">
              <span v-if="item.readBy.length" class="read-by">
                Прочитано: {{ formatReaders(item.readBy) }}
              </span>
            </div>
          </div>
        </div>
      </template>
    </RecycleScroller>
  </div>
</template>

<script setup>
import { computed } from 'vue'
import MessageSearch from './MessageSearch.vue'
import MessageService from '@/services/MessageService'

const store = useStore()
const container = ref(null)
const currentPage = ref(1)
const loading = ref(false)
const hasNewMessages = ref(false)
const isNearBottom = ref(true)

const messages = computed(() => store.state.messages.messages)
const currentUser = computed(() => store.state.auth.user)

const loadMoreMessages = async () => {
  if (loading.value) return

  loading.value = true
  try {
    const newMessages = await MessageService.fetchMessages(currentPage.value)
    if (newMessages.length) {
      store.commit('messages/PREPEND_MESSAGES', newMessages)
      currentPage.value++
    }
  } finally {
    loading.value = false
  }
}

const handleScroll = (event) => {
  const { scrollTop, scrollHeight, clientHeight } = event.target
  isNearBottom.value = scrollHeight - scrollTop - clientHeight < 100

  if (scrollTop === 0) {
    loadMoreMessages()
  }

  hasNewMessages.value = !isNearBottom.value
}

const scrollToBottom = () => {
  if (container.value) {
    container.value.scrollTop = container.value.scrollHeight
    hasNewMessages.value = false
  }
}

const groupedMessages = computed(() => {
  const groups = MessageService.groupMessagesByDate(messages.value)
  const items = []
  
  Object.entries(groups).forEach(([date, msgs]) => {
    items.push({ type: 'date', date, id: `date-${date}` })
    items.push(...msgs.map(m => ({ ...m, type: 'message' })))
  })
  
  return items
})

const getItemSize = (item) => {
  return item.type === 'date' ? 40 : 70
}

const formatDate = (date) => {
  const today = new Date().toLocaleDateString()
  const yesterday = new Date(Date.now() - 86400000).toLocaleDateString()
  
  if (date === today) return 'Сегодня'
  if (date === yesterday) return 'Вчера'
  return date
}

const scrollToMessage = (messageId) => {
  const element = document.getElementById(`message-${messageId}`)
  if (element) {
    element.scrollIntoView({ behavior: 'smooth', block: 'center' })
    element.classList.add('highlight')
    setTimeout(() => {
      element.classList.remove('highlight')
    }, 2000)
  }
}

onMounted(async () => {
  await loadMoreMessages()
  scrollToBottom()
})
</script>

<style scoped>
.messages-container {
  position: relative;
  height: 400px;
}

.messages-list {
  height: 100%;
  overflow-y: auto;
  padding: 20px;
  background: #f5f5f5;
  border-radius: 8px;
}

.new-messages-indicator {
  position: absolute;
  bottom: 20px;
  left: 50%;
  transform: translateX(-50%);
  background: #4CAF50;
  color: white;
  padding: 8px 16px;
  border-radius: 20px;
  cursor: pointer;
  box-shadow: 0 2px 4px rgba(0,0,0,0.2);
  animation: bounce 1s infinite;
}

.loading {
  text-align: center;
  padding: 10px;
  color: #666;
}

@keyframes bounce {
  0%, 100% { transform: translateX(-50%) translateY(0); }
  50% { transform: translateX(-50%) translateY(-5px); }
}

.date-separator {
  text-align: center;
  padding: 10px;
  color: #666;
  font-size: 0.9em;
  background: rgba(0,0,0,0.05);
  margin: 10px 0;
  border-radius: 4px;
}

.highlight {
  animation: highlight 2s ease-out;
}

@keyframes highlight {
  0% { background-color: #ffeb3b; }
  100% { background-color: transparent; }
}
</style>