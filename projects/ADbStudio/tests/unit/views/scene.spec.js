import { mount  } from '@vue/test-utils';
import Vuex from 'vuex';
import Scene from '@/views/Scene.vue';
import SearchField from "@/components/scene/SearchField";
import ADbView from "@/components/scene/ADbView";
import LeftPanel from "@/components/scene/LeftPanel";
import scene_store from '../../../src/store/modules/scene'
//import NodeInfo from "@/components/scene/NodeInfo";

import { localVue, i18n,scene_state } from '../TestUtils'


describe('Scene',() => {
    let wrapper;
    let actions;
    let store;
    let fadeTransitionStub; 

    beforeEach(() => {
        fadeTransitionStub = () => ({
            props: {
                opened: Boolean,
            },
            template: '<div v-if="opened" ><slot></slot></div>',
          });
        
        localVue.use(Vuex)
        actions = {
            sendCommand: jest.fn(),
        }
        store = new Vuex.Store({
            modules: {
                scene: {
                    namespaced: true,
                    actions,
                    state: scene_state,
                    getters: scene_store.getters
                }
              }
        });
        
        wrapper = mount(Scene, {
            localVue,
            i18n,
            store,
            stubs: {
                BaseMessage:  fadeTransitionStub(),
            }
        });
    })
    it('renders all elements', () => {
        expect(wrapper.find(".command").exists()).toBe(true);
        expect(wrapper.findComponent(SearchField).exists()).toBe(true);
        expect(wrapper.findComponent(ADbView).exists()).toBe(true);
        expect(wrapper.findComponent(LeftPanel).exists()).toBe(true);
        expect(wrapper.find(".reset").exists()).toBe(true);
        expect(wrapper.find(".plus").exists()).toBe(true);
        expect(wrapper.find(".minus").exists()).toBe(true);
    })
    it('sends command, fetches data and clear the input', async () => {
        wrapper.setData({command: 'test command'});
        await wrapper.find('form.command-form').trigger('submit.prevent');
        expect(wrapper.find(".command").element.value).toBe("");
        expect(actions.sendCommand).toHaveBeenCalled();
    })
    it('resets the scene after clicking reset button',async () => {
        wrapper.vm.move.x = -1;
        wrapper.vm.move.y = -1;
        wrapper.vm.zoom = -1;
        wrapper.vm.startDragCoordinates.x = -1;
        wrapper.vm.startDragCoordinates.y = -1;
        wrapper.vm.searchedWord = '-1';
        wrapper.vm.error = '-1';

        await wrapper.find('.reset').trigger('click');
        expect(wrapper.vm.move.x).toBe(0);
        expect(wrapper.vm.move.y).toBe(0);
        expect(wrapper.vm.zoom).toBe(0);
        expect(wrapper.vm.startDragCoordinates.x).toBe(0);
        expect(wrapper.vm.startDragCoordinates.x).toBe(0);
        expect(wrapper.vm.searchedWord).toBe('');
        expect(wrapper.vm.error).toBe('');
    })
    it('increases zoom after clicking plus button',async () => {
        await wrapper.find('.plus').trigger('click');
        expect(wrapper.vm.zoom).toBe(1);
    });
    it('decreases zoom after clicking minus button',async () => {
        await wrapper.find('.minus').trigger('click');
        expect(wrapper.vm.zoom).toBe(-1);
    });
    it('displays error message after searching invalid element', async () => {
        expect(wrapper.find(".search-wrap .error").exists()).toBe(false);
        await wrapper.findComponent(SearchField).vm.$emit('search','fdad');
        expect(wrapper.find(".search-wrap .error").exists()).toBe(true);
    });
    it('displays search result after searching ivalid element', async () => {
        expect(wrapper.find(".search-wrap .info").exists()).toBe(false);
        await wrapper.findComponent(SearchField).vm.$emit('search','1');
        expect(wrapper.find(".search-wrap .info").exists()).toBe(true);
        expect(wrapper.find(".search-mode").exists()).toBe(true);
        expect(wrapper.find(".node.selected").exists()).toBe(true);

    });
})
