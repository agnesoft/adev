import { mount  } from '@vue/test-utils';
import ListData from '@/components/scene/ListData.vue';

import { localVue } from '../TestUtils'


describe('ListData',() => {
    let wrapper
    beforeAll(() => {
        wrapper = mount(ListData, {
            localVue,
            propsData: {
                data: [{
                    id: -1,
                    coordinates1: {
                        x: 0.2,
                        y: 0.5,
                    },
                    coordinates2: {
                        x: 0.6,
                        y: 0.3,
                    }
                },{
                    id: -2,
                    coordinates1: {
                        x: 0.2,
                        y: 0.5,
                    },
                    coordinates2: {
                        x: 0.6,
                        y: 0.3,
                    }
                }]
            }
          });
    })
    it('renders elements ids',() => {
        expect(wrapper.findAll('li').at(0).text()).toBe("-1");
        expect(wrapper.findAll('li').at(1).text()).toBe("-2");
    })
    
})
