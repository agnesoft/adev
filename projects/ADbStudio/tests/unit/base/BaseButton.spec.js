import { shallowMount  } from '@vue/test-utils'
import BaseButton from '@/components/base/BaseButton.vue'

import { localVue } from '../TestUtils'

describe('BaseButton',() => {
    let wrapper;
    beforeAll(() => {
        wrapper = shallowMount(BaseButton, {
            localVue,
            slots: {
                default: '<div class="fake-elem"></div>',
              },
          });
    })
    it("renders fake element",() => {
        expect(wrapper.findAll(".fake-elem").length).toBe(1);
    });
    it("emits click after clicking the button",async () => {
        await  wrapper.find(".btn").trigger('click');
        expect(wrapper.emitted().click).toBeTruthy()
    });
})
