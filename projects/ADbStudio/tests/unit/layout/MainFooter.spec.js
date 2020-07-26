import { shallowMount  } from '@vue/test-utils';
import MainFooter from '@/components/layout/MainFooter.vue';

import { localVue, i18n } from '../TestUtils'


describe('MainHeader',() => {
    let wrapper
    beforeAll(() => {
        wrapper = shallowMount(MainFooter, {
            localVue,
            i18n,
          });
    })

    it('renders copyright', () => {
        expect(wrapper.find(".copyright").html()).toContain("© 2020 ADbStudio");
        expect(wrapper.find(".copyright").html()).toContain('<a href="https://www.agnesoft.com">Agnesoft</a>');
    })
})