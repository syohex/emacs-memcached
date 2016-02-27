(when window-system
  (set-frame-font "VL Gothic 12")
  (set-scroll-bar-mode 'nil)
  (tool-bar-mode 0))

(menu-bar-mode -1)

;; current-directory
(add-to-list 'load-path "/home/syohei/src/emacs-devel/modules/memcached/")
(require 'memcached)

(setq default-frame-alist
      '((width . 72) (height . 24) (top . 28) (left . 0)))
