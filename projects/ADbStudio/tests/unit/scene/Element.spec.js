import { shallowMount  } from '@vue/test-utils';
import Element from '@/components/scene/Element.vue';
import Vuex from 'vuex';

import { localVue } from '../TestUtils'


describe('Element',() => {
    let wrapper;
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
        wrapper = shallowMount(Element, {
            localVue,
            store,
            propsData: {
                element: {
                    id: 1,
                    coordinates: {
                        x: 0.2,
                        y: 0.5
                    },
                    data: {
                        foo: "bar",
                        foo1: "bar1"
                    }
                },
            },
            computed: {
                node_size() {
                    return "2.4";
                }
              }
          });
    })
    it('renders element id',() => {
        expect(wrapper.text()).toBe("1");
    })
    it('emits showData on mouse enter', async () => {
        await wrapper.find(".element").trigger("mouseenter");
        expect(wrapper.emitted().showData).toBeTruthy();
        expect(wrapper.emitted().showData[0].length).toBe(2);
        expect(wrapper.emitted().showData[0][0][0].data.foo).toBe("bar");
    })
    it('emits showData on mouse leave', async () => {
        await wrapper.find(".element").trigger("mouseleave");
        expect(wrapper.emitted().showData).toBeTruthy();
        expect(wrapper.emitted().showData.length).toBe(1);
        expect(wrapper.emitted().showData[0][0]).toBe(false);
    })
    it('fetches element data for right panel', async () => {
        await wrapper.find(".element").trigger("click");
        expect(actions.fetchElementData).toHaveBeenCalled();
    })
})
