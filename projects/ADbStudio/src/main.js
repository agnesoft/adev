import Vue from 'vue'
import './plugins/axios'
import App from './App.vue'
import router from './router'
import store from './store'
import i18n from './i18n'
import upperFirst from 'lodash/upperFirst'
import camelCase from 'lodash/camelCase'

Vue.config.productionTip = false

//----------Component Registration--------------
const requireComponent = require.context(
    // The relative path of the components folder
    './components/base',
    // Whether or not to look in subfolders
    false,
    // The regular expression used to match base component filenames
    /Base[A-Z]\w+\.(vue|js)$/
  )
  
  requireComponent.keys().forEach(fileName => {
    // Get component config
    const componentConfig = requireComponent(fileName)
  
    // Get PascalCase name of component
    const componentName = upperFirst(
      camelCase(
        // Gets the file name regardless of folder depth
        fileName
          .split('/')
          .pop()
          .replace(/\.\w+$/, '')
      )
    )
  
  
    // Register component globally
    Vue.component(
      componentName,
      // Look for the component options on `.default`, which will
      // exist if the component was exported with `export default`,
      // otherwise fall back to module's root.
      componentConfig.default || componentConfig
    )
  })  
//----------------------------------------------

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
