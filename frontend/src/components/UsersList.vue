<template>
  <div class="users-list">
    <h3>Пользователи онлайн ({{ users.length }})</h3>
    <ul>
      <li v-for="user in users" :key="user.id" class="user-item">
        <span class="user-name">{{ user.name }}</span>
        <span v-if="typingUsers[user.id]" class="typing-indicator">
          печатает...
        </span>
      </li>
    </ul>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import { useStore } from 'vuex'
import echo from '@/config/echo'

const store = useStore()
const users = ref([])
const typingUsers = ref({})

onMounted(() => {
  const channel = echo.join('online')
    .here((users) => {
      users.value = users
    })
    .joining((user) => {
      users.value.push(user)
    })
    .leaving((user) => {
      users.value = users.value.filter(u => u.id !== user.id)
      delete typingUsers.value[user.id]
    })
    .listen('UserTypingEvent', (e) => {
      if (e.isTyping) {
        typingUsers.value[e.user.id] = true
        setTimeout(() => {
          delete typingUsers.value[e.user.id]
        }, 3000)
      } else {
        delete typingUsers.value[e.user.id]
      }
    })
})
</script>

<style scoped>
.users-list {
  max-width: 300px;
  margin: 20px;
  padding: 15px;
  background: #f5f5f5;
  border-radius: 8px;
}

.user-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 8px;
  margin: 4px 0;
  background: white;
  border-radius: 4px;
}

.typing-indicator {
  font-size: 0.8em;
  color: #666;
  font-style: italic;
}
</style> 