;;; memcached.el --- libmemcached binding of Emacs Lisp

;; Copyright (C) 2016 by Syohei YOSHIDA

;; Author: Syohei YOSHIDA <syohex@gmail.com>
;; URL: https://github.com/syohex/emacs-memcached
;; Version: 0.01

;; This program is free software; you can redistribute it and/or modify
;; it under the terms of the GNU General Public License as published by
;; the Free Software Foundation, either version 3 of the License, or
;; (at your option) any later version.

;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.

;; You should have received a copy of the GNU General Public License
;; along with this program.  If not, see <http://www.gnu.org/licenses/>.

;;; Commentary:

;; libmemcached binding of Emacs Lisp

;;; Code:

(require 'cl-lib)
(require 'subr-x)
(require 'memcached-core)

;;;###autoload
(defun memcached-init (server)
  "Connect memcached `server' and return memcached state object.
`server' is like \"localhost:11211\"."
  (cl-assert (stringp server))
  (memcached-core-init server))

(defun memcached-server-add (state host port)
  "Add another server to `state'."
  (cl-assert (and (stringp host) (integerp port)))
  (memcached-core-server-add state host port))

(defun memcached-close (state)
  "Close connection."
  (memcached-core-close state))

(defsubst memcached--to-string (obj)
  (or (and (stringp obj) obj) (format "%s" obj)))

(cl-defun memcached-set (state key value &optional (expire 0))
  "Set data related to `key'. If key-value is already existed, then it is overwritten."
  (memcached-core-set state (memcached--to-string key) (memcached--to-string value) expire))

(cl-defun memcached-add (state key value &optional (expire 0))
  "Like set, but only stores if the `key` doesn't already exist."
  (memcached-core-add state (memcached--to-string key) (memcached--to-string value) expire))

(defun memcached-get (state key)
  "Get `key' object."
  (memcached-core-get state (memcached--to-string key)))

(cl-defun memcached-delete (state key &optional (expire 0))
  "Delete `key' object."
  (memcached-core-delete state (memcached--to-string key) expire))

(provide 'memcached)

;;; memcached.el ends here
