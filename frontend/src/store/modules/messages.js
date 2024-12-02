import axios from 'axios'
import echo, { updateEchoAuthToken } from '@/config/echo'

export default {
  namespaced: true,
  
  state: () => ({
    messages: [],
    loading: false,
    error: null,
    hasMore: true
  }),

  mutations: {
    SET_MESSAGES(state, messages) {
      state.messages = messages
    },
    PREPEND_MESSAGES(state, messages) {
      state.messages = [...messages, ...state.messages]
    },
    ADD_MESSAGE(state, message) {
      state.messages.push(message)
    },
    SET_LOADING(state, status) {
      state.loading = status
    },
    SET_ERROR(state, error) {
      state.error = error
    },
    SET_HAS_MORE(state, status) {
      state.hasMore = status
    }
  },

  actions: {
    initializeWebSocket({ commit, rootState }) {
      if (!rootState.auth.user) return;

      const channel = echo.private(`messages.${rootState.auth.user.id}`)
        .listen('NewMessageEvent', (event) => {
          commit('ADD_MESSAGE', event.message)
        })
        .subscribed(() => {
          commit('SET_CONNECTED', true)
        })
        .error(() => {
          commit('SET_CONNECTED', false)
        })
    },

    async sendMessage({ commit, rootState }, message) {
      if (!rootState.auth.user) {
        throw new Error('Необходима авторизация')
      }

      try {
        commit('SET_LOADING', true)
        const response = await axios.post('/api/messages', { message })
        commit('ADD_MESSAGE', response.data)
        return response.data
      } catch (error) {
        commit('SET_ERROR', error.message)
        throw error
      } finally {
        commit('SET_LOADING', false)
      }
    },

    updateAuthToken(_, token) {
      updateEchoAuthToken(token)
    }
  }
} 