import { createLocalVue } from '@vue/test-utils';
import VueI18n from 'vue-i18n';

import localesEn from '@/locales/en';
const en = { en: localesEn };

// Use a local vue
const localVue = createLocalVue();
localVue.use(VueI18n);

import VueRouter from 'vue-router';
localVue.use(VueRouter);
const router = new VueRouter({routes: [
  {path: '/'},
]});

// import BaseSelect from "@/components/base/BaseSelect.vue";
// import BaseIconSvg from "@/components/base/BaseIconSvg.vue";
// import BaseButton from "@/components/base/BaseButton.vue";
// import BaseMenu from "@/components/base/BaseMenu.vue";

// localVue.component("BaseSelect", BaseSelect);
// localVue.component("BaseIconSvg", BaseIconSvg);
// localVue.component("BaseButton", BaseButton);
// localVue.component("BaseMenu", BaseMenu);

// Create new i18n instance
module.exports = {
  localVue,
  router,
  i18n: new VueI18n({
    numberFormats: { 'en': { currency: { style: 'currency', currency: 'EUR' } } },
    locale: 'en',
    fallbackLocale: 'en',
    messages: en
  })
}