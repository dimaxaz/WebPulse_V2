import { createStore } from 'vuex'
import messages from './modules/messages'

export default createStore({
  modules: {
    messages
  }
}) 