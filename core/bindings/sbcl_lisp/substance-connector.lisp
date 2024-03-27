;;; @file substance-connector.lisp
;;; @brief Defines Steel Bank Common Lisp bindings for the Connector library
;;; @author Galen Helfter - Allegorithmic
;;; @copyright Allegorithmic. All rights reserved.

(defpackage :substance-connector
  (:use :common-lisp)
  (:export #:version
           #:initialize
           #:shutdown
           #:write-message
           #:register-callback))

;; Use a more concise package name to match more of a Common Lisp/SBCL
;; naming convention
(in-package :substance-connector)

;;; Internal module variables

;; Hash table to store the callbacks registered from outside of the module
;; Note that this same mechanism is also used in CFFI and the SBCL FFI to
;; handle foreign functions, and should work for this purpose as well.
;;
;; Set the test to the equal function for string comparisons.
(defparameter *callback-table* (make-hash-table :test 'equal))

;;; Internal functions

;; Acquire the callback stored in the hash table
(declaim (inline internal-get-callback))
(defun internal-get-callback (callback-key table)
  (gethash callback-key table))

;; Given the callback key and the callback table, calls the appropriate
;; function with the message and the context as the argument
(declaim (inline internal-call-callback))
(defun internal-call-callback (table context message-type message)
  (funcall (internal-get-callback message-type table) context message-type message))

;; Defines an alien callback that will take the message and context and
;; call the userspace functions registered with the specific message
(sb-alien::define-alien-callback connector_lisp_trampoline sb-alien:void
  ((context sb-alien:unsigned-int) (message-type sb-alien:c-string)
                                   (message sb-alien:c-string))
  (format t "Lisp has been entered~%")
  (format t "Message is ~A~%" message)
  ; Acquire the message type and call that specific callback
  (if (nth-value 1 (gethash message *callback-table*))
    ; Call procedure in hash table
    (internal-call-callback *callback-table* context message-type message)))

;; Internal mapping to the connector initialization function
(declaim (inline internal-initialize))
(sb-alien:define-alien-routine
  ("substance_connector_init" internal-initialize) sb-alien:unsigned-int)

;; Internal mapping to the connector shutdown function
(declaim (inline internal-shutdown))
(sb-alien:define-alien-routine
  ("substance_connector_shutdown" internal-shutdown) sb-alien:unsigned-int)

;; Internal representation of the connector add callback function, used to register
;; the FFI callback, but not the external event callbacks, which should go
;; through the library registration
(declaim (inline internal-add-trampoline))
(sb-alien:define-alien-routine
  ("substance_connector_add_trampoline" internal-add-trampoline)
  sb-alien:unsigned-int
  (* (sb-alien:function sb-alien:void sb-alien:unsigned-int sb-alien:c-string
                        sb-alien:c-string)))

;; Internal function, representing the remove callback function. This is used
;; to remove the FFI callbacks needed for the library.
(declaim (inline internal-remove-trampoline))
(sb-alien:define-alien-routine
  ("substance_connector_remove_trampoline" internal-remove-trampoline)
  sb-alien:unsigned-int 
  (* (sb-alien:function sb-alien:void sb-alien:unsigned-int sb-alien:c-string)))

;; Set the message-type callback to the given function
(declaim (inline internal-set-callback))
(defun internal-set-callback (callback-key table callback)
  (setf (gethash callback-key table) callback))

;;; Exposed module functions

;; Returns the current version of the library as a string
(declaim (inline version))
(sb-alien:define-alien-routine
  ("substance_connector_version" version) sb-alien:c-string)

;; Lisp module initialization function. Bootstraps components necessary for
;; the bindings as well as the internal C library.
(defun initialize ()
  (internal-initialize)
  (internal-add-trampoline connector_lisp_trampoline))

;; Shuts down the Lisp module, including all internal mechanisms as well as
;; the C module
(defun shutdown ()
  (internal-remove-trampoline connector_lisp_trampoline)
  (internal-shutdown)) ; The C module should be shut down at the end

;; Binding to write a given message out to the specified context.
(declaim (inline write-message))
(sb-alien:define-alien-routine
  ("substance_connector_write_message" write-message) sb-alien:unsigned-int
  (context sb-alien:unsigned-int)
  (message sb-alien:c-string))

;; Given a message type and a callback, registers it internally with the
;; callback table
(defun register-callback (callback-key callback)
  (internal-set-callback callback-key *callback-table* callback))
