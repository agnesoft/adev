import { mount  } from '@vue/test-utils';
import Node from '@/components/scene/Node.vue';

import { localVue } from '../TestUtils'


describe('Node',() => {
    let wrapper
    beforeAll(() => {
        wrapper = mount(Node, {
            localVue,
            propsData: {
                element: {
                    id: 1,
                    coordinates: {
                        x: 0.2,
                        y: 0.5
                    }
                },
                viewSize: {
                    width: 200,
                    height: 100
                }
            },
            computed: {
                node_size() {
                    return "2.4";
                }
              }
          });
    })
    it('renders node id',() => {
        expect(wrapper.text()).toBe("1");
    })
    it('renders node in the correct position', () => {
        let style = wrapper.attributes().style;
        expect(style).toContain('top: calc(50px - 1.2rem)');
        expect(style).toContain('left: calc(40px - 1.2rem)');
    })
    it('renders correctly (snapshot)', () => {
        expect(wrapper.element).toMatchSnapshot()
    })
})
