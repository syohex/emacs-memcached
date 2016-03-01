# memcached.el

libmemcached binding of Emacs Lisp. This package is inspired by [mruby-memcached](https://github.com/matsumoto-r/mruby-memcached)

## Screenshot

![memcached](image/memcached.png)

## Interfaces

#### `(memcached-init server)`

Connect to `server` and return memcached state instance.

#### `(memcached-server-add state host port)`

Add server(`host`:`port`) to memcached state.

#### `(memcached-close state)`

Close connection

#### `(memcached-set state key value &optional (expire 0))`

Set `value` related to `key`. Both `key` and `value` are converted into string. If `key-value` pair is already existed, then it is overwritten. `expire` is seconds.

#### `(memcached-add state key value &optional (expire 0))`

Add `value` related to `key`.

#### `(memcached-get state key)`

Get value related to `key`. If value is not found, then return `nil`.

#### `(memcached-delete state key)`

Delete value related to `key`.
