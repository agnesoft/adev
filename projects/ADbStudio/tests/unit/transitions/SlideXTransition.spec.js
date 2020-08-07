import { shallowMount  } from '@vue/test-utils'
import SlideXTransition from '@/components/transitions/SlideXTransition.vue'

describe("SlideYTransition",() => {
    it('should render fake element', async () => {
        const wrapper = shallowMount(SlideXTransition, {
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
             
        const wrapper = shallowMount(SlideXTransition, {
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
