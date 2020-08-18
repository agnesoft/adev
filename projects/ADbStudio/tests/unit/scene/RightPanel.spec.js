import { mount  } from '@vue/test-utils';
import Vuex from 'vuex';
import RightPanel from "@/components/scene/RightPanel";
import ADbView from "@/components/scene/ADbView";
import LeftPanel from "@/components/scene/LeftPanel";
import ElementData from "@/components/scene/ElementData";
import scene_store from '../../../src/store/modules/scene'
//import NodeInfo from "@/components/scene/NodeInfo";

import { localVue, i18n,scene_state } from '../TestUtils'


describe('Scene',() => {
    let wrapper;
    let store;
    let slideTransitionStub; 
    let transtionStub;

    beforeEach(() => {
        slideTransitionStub = () => ({
            props: {
                opened: Boolean,
            },
            template: '<div v-if="opened" ><slot></slot></div>',
          });
          transtionStub = () => ({
              template: '<div><slot></slot></div>',
            });
        
        localVue.use(Vuex)
        store = new Vuex.Store({
            modules: {
                scene: {
                    namespaced: true,
                    actions: scene_store.actions,
                    mutations: scene_store.mutations,
                    state: scene_state,
                    getters: scene_store.getters
                }
              }
        });
        
        wrapper = mount(RightPanel, {
            localVue,
            i18n,
            store,
            stubs: {
                SlideXTransition:  slideTransitionStub(),
                transition:  transtionStub(),
                "transition-group":  transtionStub(),
            }
        });
    })
    it('renders all elements', () => {
        expect(wrapper.find(".btn-primary").exists()).toBe(true);
        expect(wrapper.find(".btn-primary").text()).toBe("Hide all data");
        expect(wrapper.findAll(".base-box").length).toBe(2);
    })
    it('renders correctly (snapshot)', () => {
        expect(wrapper.element).toMatchSnapshot()
    })
    it('renders boxes correctly',() => {        
        let boxes = wrapper.findAll(".base-box");
        expect(boxes.at(0).find('.title').text()).toBe('Node 1');
        let firsttable = boxes.at(0).find('.content').text();
        expect(firsttable).toBe('name Jananumber 18');

        expect(boxes.at(1).find('.title').text()).toBe('Edge -2');
        let secondtable = boxes.at(1).find('.content').text();
        expect(secondtable).toBe('foo bar');
    })
    it('closes box after clicking close box button', async () => {
        let boxes = wrapper.findAll(".base-box");
        await boxes.at(0).find('.controls>.btn').trigger('click');
        expect(wrapper.findAll(".base-box").length).toBe(1);
    })
    it('closes all boxes after clicking close all button', async () => {
        await wrapper.find(".btn-primary").trigger('click');
        expect(wrapper.findAll(".base-box").length).toBe(0);
    })

   
})
