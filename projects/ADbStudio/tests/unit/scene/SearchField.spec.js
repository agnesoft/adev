import { mount  } from '@vue/test-utils';
import SearchField from '@/components/scene/SearchField.vue';
import BaseButton from '@/components/base/BaseButton.vue';

import { localVue,i18n } from '../TestUtils'


describe('SearchField',() => {
    let wrapper
    beforeEach(() => {
        wrapper = mount(SearchField, {
            localVue,
            i18n
          });
    })
    it('renders BaseButton',() => {      
        expect(wrapper.find(".btn").exists()).toBe(true);
        expect(wrapper.findComponent(BaseButton).exists()).toBe(true);
    });
    it('renders input field on click, on second click hides it', async () => {
        expect(wrapper.find("input").exists()).toBe(false);        
        await  wrapper.find(".btn").trigger('click');
        expect(wrapper.find("input").exists()).toBe(true);
        await  wrapper.find(".btn").trigger('click');
        expect(wrapper.find("input").exists()).toBe(false);
    });
    it('emit search value from input after clicking the button',async () => {  
        await  wrapper.find(".btn").trigger('click');
        wrapper.find('input').element.value = "foo";
        wrapper.find('input').trigger('input');
        await  wrapper.find(".btn").trigger('click');
        expect(wrapper.emitted().search).toBeTruthy();
        expect(wrapper.emitted().search[0]).toEqual(['foo']);
        expect(wrapper.find("input").exists()).toBe(false);
    });
})
