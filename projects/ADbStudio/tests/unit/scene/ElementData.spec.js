import { shallowMount  } from '@vue/test-utils';
import ElementData from '@/components/scene/ElementData.vue';

import { localVue } from '../TestUtils'


describe('ElementData',() => {
    let wrapper
    beforeAll(() => {
        wrapper = shallowMount(ElementData, {
            localVue,
            propsData: {
                data: {
                    foo: "bar",
                    foo1: "bar1"
                },
            },
            computed: {
                node_size() {
                    return "2.4";
                }
              }
          });
    })
    it('renders table with data',() => {
        expect(wrapper.find("table")).toBeTruthy();
        expect(wrapper.findAll("tr").length).toBe(2);
        let cells = wrapper.findAll("td");
        expect(cells.length).toBe(4);
        expect(cells.at(0).text()).toBe('foo');
        expect(cells.at(1).text()).toBe('bar');
        expect(cells.at(2).text()).toBe('foo1');
        expect(cells.at(3).text()).toBe('bar1');
    })
})
