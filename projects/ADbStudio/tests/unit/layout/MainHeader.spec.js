import { shallowMount  } from '@vue/test-utils';
import MainHeader from '@/components/layout/MainHeader.vue';
//import LanguageSwitcher from "@/components/layout/LanguageSwitcher";

import { localVue, i18n } from '../TestUtils'


describe('MainHeader',() => {
    let wrapper
    beforeAll(() => {
        wrapper = shallowMount(MainHeader, {
            localVue,
            i18n,
          });
    })
    it('renders title and components', () => {
        //expect(wrapper.findComponent(LanguageSwitcher).exists()).toBe(true);
        expect(wrapper.find("header").exists()).toBe(true);
        expect(wrapper.find(".logo").text()).toBe("ADbStudio");
    })
})