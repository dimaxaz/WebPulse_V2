<template>
  <div id="app">
    <h1>WebPulse Messaging</h1>
    
    <template v-if="isAuthenticated">
      <div class="connection-status" :class="{ connected }">
        {{ connected ? 'Подключено' : 'Отключено' }}
      </div>
      <div class="main-container">
        <UsersList />
        <div class="chat-container">
          <VirtualMessagesList />
          <MessageForm />
        </div>
      </div>
    </template>
    <AuthForm v-else />
  </div>
</template>

<script setup>
import { onMounted, computed } from 'vue'
import { useStore } from 'vuex'
import MessageForm from './components/MessageForm.vue'
import AuthForm from './components/AuthForm.vue'
import UsersList from './components/UsersList.vue'
import VirtualMessagesList from './components/VirtualMessagesList.vue'

const store = useStore()

const isAuthenticated = computed(() => store.getters['auth/isAuthenticated'])
const connected = computed(() => store.state.messages.connected)

onMounted(() => {
    if (isAuthenticated.value) {
        store.dispatch('messages/initializeWebSocket')
    }
})
</script>

<style>
.connection-status {
    padding: 5px 10px;
    margin: 10px;
    border-radius: 4px;
    text-align: center;
    background-color: #ff4444;
    color: white;
}

.connection-status.connected {
    background-color: #4CAF50;
}

.main-container {
  display: flex;
  max-width: 1200px;
  margin: 0 auto;
  padding: 20px;
}

.chat-container {
  flex: 1;
  margin-left: 20px;
  display: flex;
  flex-direction: column;
  gap: 20px;
}
</style> 