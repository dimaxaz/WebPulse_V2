<template>
  <div class="auth-form">
    <div v-if="error" class="error">{{ error }}</div>
    
    <form @submit.prevent="handleSubmit">
      <template v-if="isRegister">
        <div class="form-group">
          <input 
            v-model="form.name" 
            type="text" 
            placeholder="Имя"
            required
          >
        </div>
      </template>

      <div class="form-group">
        <input 
          v-model="form.email" 
          type="email" 
          placeholder="Email"
          required
        >
      </div>

      <div class="form-group">
        <input 
          v-model="form.password" 
          type="password" 
          placeholder="Пароль"
          required
        >
      </div>

      <template v-if="isRegister">
        <div class="form-group">
          <input 
            v-model="form.password_confirmation" 
            type="password" 
            placeholder="Подтвердите пароль"
            required
          >
        </div>
      </template>

      <button type="submit" :disabled="loading">
        {{ loading ? 'Загрузка...' : (isRegister ? 'Регистрация' : 'Вход') }}
      </button>

      <p class="toggle-auth">
        {{ isRegister ? 'Уже есть аккаунт?' : 'Нет аккаунта?' }}
        <a href="#" @click.prevent="toggleAuthMode">
          {{ isRegister ? 'Войти' : 'Зарегистрироваться' }}
        </a>
      </p>
    </form>
  </div>
</template>

<script setup>
import { ref, computed } from 'vue'
import { useStore } from 'vuex'

const store = useStore()
const isRegister = ref(false)

const form = ref({
  name: '',
  email: '',
  password: '',
  password_confirmation: ''
})

const loading = computed(() => store.state.auth.loading)
const error = computed(() => store.state.auth.error)

const handleSubmit = async () => {
  try {
    if (isRegister.value) {
      await store.dispatch('auth/register', form.value)
    } else {
      await store.dispatch('auth/login', {
        email: form.value.email,
        password: form.value.password
      })
    }
  } catch (error) {
    console.error('Ошибка аутентификации:', error)
  }
}

const toggleAuthMode = () => {
  isRegister.value = !isRegister.value
  form.value = {
    name: '',
    email: '',
    password: '',
    password_confirmation: ''
  }
}
</script>

<style scoped>
.auth-form {
  max-width: 400px;
  margin: 20px auto;
  padding: 20px;
}

.form-group {
  margin-bottom: 15px;
}

input {
  width: 100%;
  padding: 8px;
  border: 1px solid #ddd;
  border-radius: 4px;
}

button {
  width: 100%;
  padding: 10px;
  background-color: #4CAF50;
  color: white;
  border: none;
  border-radius: 4px;
  cursor: pointer;
}

button:disabled {
  background-color: #cccccc;
}

.error {
  color: red;
  margin-bottom: 15px;
}

.toggle-auth {
  margin-top: 15px;
  text-align: center;
}

.toggle-auth a {
  color: #4CAF50;
  text-decoration: none;
}
</style> 