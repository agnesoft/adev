'use strict';

test('Can mount app', () => {
  document.body.innerHTML =
    '<div id="app">' +
    '</div>';

  require('../../src/main');

  const pElement = document.getElementById('app');
  expect(pElement).toBeTruthy();
});