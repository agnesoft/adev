import { shallowMount  } from '@vue/test-utils';
import Node from '@/components/scene/Node.vue';

import { localVue } from '../TestUtils'


describe('Node',() => {
    let wrapper
    beforeAll(() => {
        wrapper = shallowMount(Node, {
            localVue,
            propsData: {
                params: {
                    id: 1,
                    coordinates: {
                        x: 0.2,
                        y: 0.5
                    }
                }
            }
          });
    })
    it('renders node id',() => {
        expect(wrapper.text()).toBe("1");
    })
    it('renders node in the correct position', () => {
        let style = wrapper.attributes().style;
        expect(style).toContain('top: 50%');
        expect(style).toContain('left: 20%');
    })
})
