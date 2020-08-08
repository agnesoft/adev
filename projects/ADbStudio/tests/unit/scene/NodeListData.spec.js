import { mount  } from '@vue/test-utils';
import Vuex from 'vuex';
import NodeListData from "@/components/scene/NodeListData";
import scene_store from '../../../src/store/modules/scene'

import { localVue, i18n,scene_state } from '../TestUtils'


describe('NodeListData',() => {
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
        
        wrapper = mount(NodeListData, {
            localVue,
            i18n,
            store,
        });
    })
    it('renders all elements', () => {
        expect(wrapper.findAll('li').at(0).text()).toBe("1");
        expect(wrapper.findAll('li').at(1).text()).toBe("2");
    })
})
