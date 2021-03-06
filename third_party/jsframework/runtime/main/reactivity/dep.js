/* eslint-disable */

import { remove } from '../../utils/index.ts';

let uid = 0;

/**
 * A dep is an observable that can have multiple directives subscribing to it.
 * @constructor
 */
export default function Dep () {
  this.id = uid++;
  this.subs = [];
}

// The current target watcher being evaluated.
// This is globally unique because there could be only one watcher being evaluated at any time.
Dep.target = null;
let targetStack = [];

export function pushTarget (_target) {
  if (Dep.target) {
    targetStack.push(Dep.target);
  }
  Dep.target = _target;
}

export function popTarget () {
  Dep.target = targetStack.pop();
}

export function resetTarget () {
  Dep.target = null;
  targetStack = [];
}

/**
 * Add a directive subscriber.
 * @param {Directive} sub
 */
Dep.prototype.addSub = function (sub) {
  this.subs.push(sub);
}

/**
 * Remove a directive subscriber.
 * @param {Directive} sub
 */
Dep.prototype.removeSub = function (sub) {
  remove(this.subs, sub);
}

/**
 * Add self as a dependency to the target watcher.
 */
Dep.prototype.depend = function () {
  if (Dep.target) {
    Dep.target.addDep(this);
  }
}

/**
 * Notify all subscribers of a new value.
 */
Dep.prototype.notify = function () {
  // Stabilize the subscriber list first.
  const subs = this.subs.slice();
  for (let i = 0, l = subs.length; i < l; i++) {
    subs[i].update();
  }
}
