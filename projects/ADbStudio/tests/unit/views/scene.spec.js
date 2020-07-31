import { shallowMount  } from '@vue/test-utils';
import Vuex from 'vuex';
import Scene from '@/views/Scene.vue';
import SearchField from "@/components/scene/SearchField";
import ADbView from "@/components/scene/ADbView";
import scene_store from '../../../src/store/modules/scene'
//import NodeInfo from "@/components/scene/NodeInfo";

import { localVue, i18n } from '../TestUtils'


describe('Scene',() => {
    let wrapper;
    let actions;
    let store;

    beforeAll(() => {
        
        localVue.use(Vuex)
        actions = {
            sendCommand: jest.fn(),
        }
        store = new Vuex.Store({
            modules: {
                scene: {
                    namespaced: true,
                    actions,
                    getters: scene_store.getters
                }
              }
        });
        
        wrapper = shallowMount(Scene, {
            localVue,
            i18n,
            store
        });
    })
    it('renders all elements', () => {
        expect(wrapper.find(".command").exists()).toBe(true);
        expect(wrapper.findComponent(SearchField).exists()).toBe(true);
        expect(wrapper.findComponent(ADbView).exists()).toBe(true);
    })
    it('sends command, fetches data and clear the input', async () => {
        wrapper.setData({command: 'test command'});
        await wrapper.find('form.command-form').trigger('submit.prevent');
        expect(wrapper.find(".command").element.value).toBe("");
        expect(actions.sendCommand).toHaveBeenCalled()
    })
})
