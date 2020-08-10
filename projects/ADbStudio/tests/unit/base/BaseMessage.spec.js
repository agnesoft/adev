import { mount  } from '@vue/test-utils'
import BaseMessage from '@/components/base/BaseMessage.vue'
import BaseButton from '@/components/base/BaseButton.vue'

import { localVue,i18n } from '../TestUtils'

describe('BaseButton',() => {
    let wrapper;
    let fadeTransitionStub; 
    beforeAll(() => {
        fadeTransitionStub = () => ({
            props: {
                opened: Boolean,
            },
            template: '<div v-if="opened" ><slot></slot></div>',
          });
        wrapper = mount(BaseMessage, {
            localVue,

            slots: {
                default: '<div class="fake-elem"></div>',
              },
            i18n,
            stubs: {
                FadeTransition:  fadeTransitionStub(),
            }
          });
    })
    it("renders fake element and button",() => {
        expect(wrapper.findAll(".fake-elem").length).toBe(1);
        expect(wrapper.findComponent(BaseButton).exists()).toBe(true);
    });
    it("emits 'close' after clicking the button",async () => {
        await  wrapper.find('.close').trigger('click');
        expect(wrapper.emitted().close).toBeTruthy()
    });
})
