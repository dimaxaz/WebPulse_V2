import axios from 'axios'

export default {
    namespaced: true,

    state: () => ({
        user: null,
        token: localStorage.getItem('token'),
        loading: false,
        error: null
    }),

    mutations: {
        SET_USER(state, user) {
            state.user = user
        },
        SET_TOKEN(state, token) {
            state.token = token
            if (token) {
                localStorage.setItem('token', token)
                axios.defaults.headers.common['Authorization'] = `Bearer ${token}`
            } else {
                localStorage.removeItem('token')
                delete axios.defaults.headers.common['Authorization']
            }
        },
        SET_LOADING(state, status) {
            state.loading = status
        },
        SET_ERROR(state, error) {
            state.error = error
        }
    },

    actions: {
        async register({ commit }, credentials) {
            try {
                commit('SET_LOADING', true)
                const response = await axios.post('/api/register', credentials)
                commit('SET_USER', response.data.user)
                commit('SET_TOKEN', response.data.token)
                return response.data
            } catch (error) {
                commit('SET_ERROR', error.response?.data?.message || 'Ошибка регистрации')
                throw error
            } finally {
                commit('SET_LOADING', false)
            }
        },

        async login({ commit }, credentials) {
            try {
                commit('SET_LOADING', true)
                const response = await axios.post('/api/login', credentials)
                commit('SET_USER', response.data.user)
                commit('SET_TOKEN', response.data.token)
                return response.data
            } catch (error) {
                commit('SET_ERROR', error.response?.data?.message || 'Ошибка входа')
                throw error
            } finally {
                commit('SET_LOADING', false)
            }
        },

        async logout({ commit }) {
            try {
                await axios.post('/api/logout')
                commit('SET_USER', null)
                commit('SET_TOKEN', null)
            } catch (error) {
                console.error('Ошибка при выходе:', error)
            }
        }
    },

    getters: {
        isAuthenticated: state => !!state.token,
        currentUser: state => state.user
    }
} 