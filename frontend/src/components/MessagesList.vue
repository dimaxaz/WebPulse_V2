<template>
  <div class="messages-list" ref="messagesList">
    <div 
      v-for="message in messages" 
      :key="message.id" 
      class="message"
      :class="{ 'my-message': isMyMessage(message) }"
    >
      <div class="message-content">
        <div class="message-header">
          <span class="author">{{ message.user.name }}</span>
          <span class="time">{{ formatTime(message.created_at) }}</span>
        </div>
        <div class="message-text">{{ message.content }}</div>
        <div class="message-status">
          <span v-if="message.readBy.length" class="read-by">
            Прочитано: {{ formatReaders(message.readBy) }}
          </span>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, watch } from 'vue'
import { useStore } from 'vuex'
import NotificationService from '@/services/NotificationService'
import axios from 'axios'

const store = useStore()
const messagesList = ref(null)
const messages = computed(() => store.state.messages.messages)
const currentUser = computed(() => store.state.auth.user)

const isMyMessage = (message) => {
  return message.user_id === currentUser.value.id
}

const formatTime = (timestamp) => {
  return new Date(timestamp).toLocaleTimeString()
}

const formatReaders = (readers) => {
  return readers.map(reader => reader.name).join(', ')
}

const markAsRead = async (message) => {
  if (!isMyMessage(message) && !message.readBy.some(r => r.id === currentUser.value.id)) {
    await axios.post(`/api/messages/${message.id}/read`)
  }
}

const scrollToBottom = () => {
  if (messagesList.value) {
    messagesList.value.scrollTop = messagesList.value.scrollHeight
  }
}

watch(() => messages.value, async (newMessages, oldMessages) => {
  if (newMessages.length > oldMessages.length) {
    const newMessage = newMessages[newMessages.length - 1]
    
    if (!isMyMessage(newMessage)) {
      NotificationService.notify('Новое сообщение', {
        body: `${newMessage.user.name}: ${newMessage.content}`
      })
      await markAsRead(newMessage)
    }
    
    scrollToBottom()
  }
}, { deep: true })

onMounted(() => {
  scrollToBottom()
})
</script>

<style scoped>
.messages-list {
  height: 400px;
  overflow-y: auto;
  padding: 20px;
  background: #f5f5f5;
  border-radius: 8px;
}

.message {
  margin: 10px 0;
  max-width: 70%;
}

.my-message {
  margin-left: auto;
}

.message-content {
  background: white;
  padding: 10px;
  border-radius: 8px;
  box-shadow: 0 1px 2px rgba(0,0,0,0.1);
}

.message-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 5px;
}

.author {
  font-weight: bold;
}

.time {
  font-size: 0.8em;
  color: #999;
}

.message-text {
  margin-bottom: 5px;
}

.message-status {
  font-size: 0.8em;
  color: #999;
}
</style>