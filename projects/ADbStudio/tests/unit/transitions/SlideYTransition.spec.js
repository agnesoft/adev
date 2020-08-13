import { shallowMount  } from '@vue/test-utils'
import SlideYTransition from '@/components/transitions/SlideYTransition.vue'

describe("SlideYTransition",() => {
    it('should render fake element', async () => {
        const wrapper = shallowMount(SlideYTransition, {
            slots: {
              default: '<div class="fake-elem"></div>'
            },
            propsData: {
                opened: true,
            }
          });
          expect(wrapper.findAll(".fake-elem").length).toBe(1);
      })
      
      it('should not render fake element', async () => {
             
        const wrapper = shallowMount(SlideYTransition, {
            slots: {
              default: '<div class="fake-elem"></div>'
            },
            propsData: {
                opened: false,
            }
            
          });
      
          expect(wrapper.findAll(".fake-elem").length).toBeFalsy();
      })
})
