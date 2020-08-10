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
 import BaseIcon from "@/components/base/BaseIcon.vue";
 import BaseButton from "@/components/base/BaseButton.vue";
 import BaseMessage from "@/components/base/BaseMessage.vue";

// localVue.component("BaseSelect", BaseSelect);
 localVue.component("BaseIcon", BaseIcon);
 localVue.component("BaseButton", BaseButton);
 localVue.component("BaseMessage", BaseMessage);


 const scene_state = {
    nodes: [{
        id: 1,
        coordinates: {
            x: 0.2,
            y: 0.2,
        }
        
    },{
        id: 2,
        coordinates: {
            x: 0.7,
            y: 0.7,
        }
    }],
    edges: [{
        id: -1,
        coordinates1: {
            x: 0.2,
            y: 0.2,
        },
        coordinates2: {
            x: 0.7,
            y: 0.7,
        }
    }],
};


// Create new i18n instance
module.exports = {
  localVue,
  router,
  scene_state,
  i18n: new VueI18n({
    numberFormats: { 'en': { currency: { style: 'currency', currency: 'EUR' } } },
    locale: 'en',
    fallbackLocale: 'en',
    messages: en
  })
}
