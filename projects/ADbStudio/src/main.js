import Vue from 'vue'
import './plugins/axios'
import App from './App.vue'
import router from './router'
import store from './store'
import i18n from './i18n'

Vue.config.productionTip = false


//------------Language Switching ---------------
// use beforeEach route guard to set the language
router.beforeEach((to, from, next) => {

  // use the language from the routing param or default language
  let language = to.params.lang;
  if (!language) {
    language = 'en'
  }

  // set the current language for i18n.
  i18n.locale = language
  next()
})
//----------------------------------------------

new Vue({
  router,
  store,
  i18n,
  render: h => h(App)
}).$mount('#app')
