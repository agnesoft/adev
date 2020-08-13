import { mount  } from '@vue/test-utils'
import BaseBox from '@/components/base/BaseBox.vue'

import { localVue } from '../TestUtils'

describe('BaseBox',() => {
    let wrapper;
    let slideTransitionStub; 

    beforeEach(() => {
        
        slideTransitionStub = () => ({
            props: {
                opened: Boolean,
            },
            template: '<div v-if="opened" ><slot></slot></div>',
          });
        wrapper = mount(BaseBox, {
            localVue,
            slots: {
                title: '<div class="fake-title">title</div>',
                content: '<div class="fake-content">content</div>',
                controls: '<div class="fake-controls">controls</div>',
            },
            stubs: {
                SlideYTransition:  slideTransitionStub(),
            }
          });
    })
    it("renders fake title, fake controls and toggle button, does not render fake content",() => {
        expect(wrapper.findAll(".fake-title").length).toBe(1);
        expect(wrapper.find(".fake-title").text()).toBe("title");
        expect(wrapper.findAll(".fake-controls").length).toBe(1);
        expect(wrapper.find(".fake-controls").text()).toBe("controls");
        expect(wrapper.find(".toggle-button").exists()).toBeTruthy();
        
        expect(wrapper.find(".fake-content").exists()).toBeFalsy();
    });
    it("shows content after clicking toggle button",async () => {
        await  wrapper.find(".toggle-button").trigger('click');
        expect(wrapper.find(".fake-content").exists()).toBeTruthy();
        expect(wrapper.find(".fake-content").text()).toBe("content")
    });
    it("renders fake content and does not render toggle button, if is not closable", async () => {
        await wrapper.setProps({closable: false});
        expect(wrapper.find(".fake-content").exists()).toBeTruthy();
        expect(wrapper.find(".fake-content").text()).toBe("content");
        expect(wrapper.find(".toggle-button").exists()).toBeFalsy();
    });
})
