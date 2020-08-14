import { shallowMount  } from '@vue/test-utils';
import Elements from '@/components/scene/Elements.vue';
import Vuex from 'vuex';

import { localVue } from '../TestUtils'


describe('Elements',() => {
    let wrapper
    let actions;
    let store;
    beforeEach(() => {
        
        localVue.use(Vuex);
        actions = {
            fetchElementData: jest.fn(),
        }
        store = new Vuex.Store({
            modules: {
                scene: {
                    namespaced: true,
                    actions,
                }
              }
        });

        wrapper = shallowMount(Elements, {
            localVue,
            store,
            propsData: {
                elements: [{
                    id: 1,
                    coordinates: {
                        x: 0.2,
                        y: 0.5
                    },
                    data: {
                        foo: "bar",
                        foo1: "bar1"
                    }
                },{
                    id: 2,
                    coordinates: {
                        x: 0.2,
                        y: 0.5
                    },
                },{
                    id: 3,
                    coordinates: {
                        x: 0.2,
                        y: 0.5
                    },
                },]
            },
        });
    })
    it('emits showData on mouse enter', async () => {
        await wrapper.find(".elements").trigger("mouseenter");
        expect(wrapper.emitted().showData).toBeTruthy();
        expect(wrapper.emitted().showData[0].length).toBe(2);
        expect(wrapper.emitted().showData[0][0][0].data.foo).toBe("bar");
    })
    it('emits showData on mouse leave', async () => {
        await wrapper.find(".elements").trigger("mouseleave");
        expect(wrapper.emitted().showData).toBeTruthy();
        expect(wrapper.emitted().showData.length).toBe(1);
        expect(wrapper.emitted().showData[0][0]).toBe(false);
    })
    it('fetches elements data for right panel', async () => {
        await wrapper.find(".elements").trigger("click");
        expect(actions.fetchElementData).toHaveBeenCalledTimes(3);
    })

})
