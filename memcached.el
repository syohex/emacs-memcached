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
  (cl-assert (stringp server))
  (memcached-core-init server))

(defun memcached-close (mst)
  (memcached-core-close mst))

(defsubst memcached--to-string (obj)
  (or (and (stringp obj) obj) (format "%s" obj)))

(defun memcached-set (mst key value &optional expire)
  (memcached-core-set mst (memcached--to-string key) (memcached--to-string value) (or expire 0)))

(defun memcached-get (mst key)
  (memcached-core-get mst (memcached--to-string key)))

(provide 'memcached)

;;; memcached.el ends here
