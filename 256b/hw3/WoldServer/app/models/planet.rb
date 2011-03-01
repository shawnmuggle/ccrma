class Planet < ActiveRecord::Base
  
  has_many :trees
  
  # tell plist library how to render a message in a plist.
  # puts all the relevant information in a hash, then renders that as a plist fragment.
  # the fragments are collected and wrapped in the plist boilerplate in the controller.
  def to_plist_node
    hash = { :red => r, :green => g, :blue => b, :texture => texture }
    Plist::Emit.dump(hash, false)
  end

end