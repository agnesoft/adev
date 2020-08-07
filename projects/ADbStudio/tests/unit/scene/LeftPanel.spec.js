import { mount  } from '@vue/test-utils';
import LeftPanel from '@/components/scene/LeftPanel.vue';

import scene_store from '../../../src/store/modules/scene'
import { localVue, i18n,scene_state } from '../TestUtils'
import Vuex from 'vuex';
import BaseButton from "@/components/base/BaseButton.vue";


describe('LeftPanel',() => {
    let wrapper;
    let store;
    let transitionStub; 
    let slideTransitionStub; 

    beforeEach(() => {
        slideTransitionStub = () => ({
            props: {
                opened: Boolean,
            },
            template: '<div v-if="opened" ><slot></slot></div>',
          });
        transitionStub = () => ({
            render: function(h) {
              return this.$options._renderChildren
            }
          });
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
        
        wrapper = mount(LeftPanel, {
            localVue,
            i18n,
            store,
            stubs: {
                transition: transitionStub(),
                SlideXTransition:  slideTransitionStub(),
              }
        });
    })
    it('renders tab buttons',() => {
        let buttons = wrapper.findAll(".tab-button");
        expect(buttons).toHaveLength(3);
        expect(buttons.at(0).attributes('title')).toBe("List of Nodes");
        expect(buttons.at(1).attributes('title')).toBe("List of Edges");
        expect(buttons.at(2).attributes('title')).toBe("Raw Data");
    });

    it("renders tab content after button is clicked",async () => {
        expect(wrapper.find('.tab-box').exists()).toBeFalsy();
        let buttons = wrapper.findAllComponents(BaseButton);
        await buttons.at(0).trigger('click');
        expect(wrapper.find('.tab-box').exists()).toBeTruthy();
    })
    it("changes tab content after another button is clicked", async () => {
        let buttons = wrapper.findAllComponents(BaseButton);
        await buttons.at(0).trigger('click');
        expect(wrapper.find('.tab-header').text()).toBe("List of Nodes");
        await buttons.at(1).trigger('click');
        expect(wrapper.find('.tab-header').text()).toBe("List of Edges");
        await buttons.at(2).trigger('click');
        expect(wrapper.find('.tab-header').text()).toBe("Raw Data");
    })
    it("hides tab content after clicking the same tab button", async () => {
        let buttons = wrapper.findAllComponents(BaseButton);
        await buttons.at(0).trigger('click');
        await buttons.at(0).trigger('click');
        expect(wrapper.find('.tab-box').exists()).toBeFalsy();
    })
})
