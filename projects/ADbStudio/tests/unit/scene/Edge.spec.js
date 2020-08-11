import { mount  } from '@vue/test-utils';
import Edge from '@/components/scene/Edge.vue';

import { localVue } from '../TestUtils'


describe('Edge',() => {
    let wrapper
    beforeAll(() => {
        wrapper = mount(Edge, {
            localVue,
            propsData: {
                element: {
                    id: -1,
                    coordinates1: {
                        x: 0.2,
                        y: 0.5,
                    },
                    coordinates2: {
                        x: 0.6,
                        y: 0.3,
                    }
                },
                viewSize: {
                    width: 200,
                    height: 100
                }
            }
          });
    })
    it('renders edge id',() => {
        expect(wrapper.text()).toBe("-1");
    })
    
    it('renders edge in the correct position', () => {
        let style = wrapper.attributes().style;
        expect(style).toContain('--left_pos: 38.76');
        expect(style).toContain('--top_pos: 40');
        expect(style).toContain('--length: 82.46');
        expect(style).toContain('--angle: 6.03');
    })
})
