import Vue from 'vue'
import Vuex from 'vuex'

Vue.use(Vuex)

import Scene from "./modules/scene";

export default new Vuex.Store({
    strict: process.env.NODE_ENV !== 'production',
    state: {
    },
    mutations: {
    },
    actions: {
    },
    modules: {
        scene: Scene,
    }
})
