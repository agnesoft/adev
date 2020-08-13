import { mount  } from '@vue/test-utils';
import Vuex from 'vuex';
import ADbView from '@/components/scene/ADbView.vue';
import scene_store from '../../../src/store/modules/scene'

import { localVue,scene_state } from '../TestUtils'


describe('ADbView',() => {
    let wrapper;
    let store;

    beforeAll(() => {
        localVue.use(Vuex)
        
        store = new Vuex.Store({
            modules: {
                scene: {
                    namespaced: true,
                    state: scene_state,
                    getters: scene_store.getters
                }
              }
        });
        wrapper = mount(ADbView, {
            localVue,
            store
          });
    })
    it('renders nodes and edges',() => {
        expect(wrapper.find('.node').exists()).toBe(true);
        expect(wrapper.find('.edge').exists()).toBe(true);
    })
})
