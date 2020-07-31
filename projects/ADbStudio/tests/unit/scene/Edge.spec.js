import { shallowMount  } from '@vue/test-utils';
import Edge from '@/components/scene/Edge.vue';

import { localVue } from '../TestUtils'


describe('Edge',() => {
    let wrapper
    beforeAll(() => {
        wrapper = shallowMount(Edge, {
            localVue,
            propsData: {
                params: {
                    id: -1,
                    coordinates: {
                        x: 0.2,
                        y: 0.5
                    }
                }
            }
          });
    })
    it('renders edge id',() => {
        expect(wrapper.text()).toBe("-1");
    })
})
