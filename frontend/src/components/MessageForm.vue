<template>
  <div class="message-form">
    <input v-model="message" type="text" placeholder="Введите сообщение">
    <button @click="sendMessage">Отправить</button>
  </div>
</template>

<script setup>
import { ref } from 'vue'

const message = ref('')

const sendMessage = () => {
  console.log('Отправка сообщения:', message.value)
  // Здесь будет логика отправки
}
</script> 