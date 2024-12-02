<template>
  <div class="message-form">
    <div v-if="error" class="error">{{ error }}</div>
    
    <div class="form-container">
      <input 
        v-model="message" 
        type="text" 
        placeholder="Введите сообщение"
        :disabled="loading"
        @input="handleTyping"
      >
      <button 
        @click="sendMessage"
        :disabled="loading || !message.trim()"
      >
        {{ loading ? 'Отправка...' : 'Отправить' }}
      </button>
    </div>

    <div class="messages-list">
      <div v-for="msg in messages" :key="msg.id" class="message">
        {{ msg.text }}
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, computed } from 'vue'
import { useStore } from 'vuex'
import axios from 'axios'

const store = useStore()
const message = ref('')
let typingTimeout = null

const loading = computed(() => store.state.messages.loading)
const error = computed(() => store.state.messages.error)
const messages = computed(() => store.state.messages.messages)

const handleTyping = () => {
  if (typingTimeout) {
    clearTimeout(typingTimeout)
  }

  axios.post('/api/typing', { isTyping: true })
  
  typingTimeout = setTimeout(() => {
    axios.post('/api/typing', { isTyping: false })
  }, 2000)
}

const sendMessage = async () => {
  if (!message.value.trim()) return
  
  try {
    await store.dispatch('messages/sendMessage', message.value)
    message.value = '' // очищаем поле после успешной отправки
  } catch (error) {
    console.error('Ошибка при отправке:', error)
  }
}
</script>

<style scoped>
.message-form {
  max-width: 600px;
  margin: 20px auto;
  padding: 20px;
}

.form-container {
  display: flex;
  gap: 10px;
  margin-bottom: 20px;
}

input {
  flex: 1;
  padding: 8px;
  border: 1px solid #ddd;
  border-radius: 4px;
}

button {
  padding: 8px 16px;
  background-color: #4CAF50;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
}

button:disabled {
  background-color: #cccccc;
  cursor: not-allowed;
}

.error {
  color: red;
  margin-bottom: 10px;
}

.messages-list {
  margin-top: 20px;
}

.message {
  padding: 10px;
  margin: 5px 0;
  background-color: #f5f5f5;
  border-radius: 4px;
}
</style> 